/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.RectF;
import android.util.Log;
import android.util.Size;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListPopupWindow;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.qualcomm.qti.fr_camera.R;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateTranslator;
import com.qualcomm.qti.fr_camera.fr.Face;
import com.qualcomm.qti.fr_camera.fr.FacePreferences;
import com.qualcomm.qti.fr_camera.fr.UserDatabase;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Objects;

/**
 * This class manages a {@link FrameLayout}, creating and destroying the {@link View}s necessary
 * to show the output of face recognition
 */
public abstract class FaceLayoutManager {
    /**
     * The supported operation modes for this {@link FaceLayoutManager}
     */
    public enum Mode {
        /**
         * Bounding boxes will be rendered for all detected faces. Name tags will be rendered for
         * recognized faces, but will not be actionable.
         */
        PREVIEW,

        /**
         * Bounding boxes will not be rendered. Name tags will be rendered for all faces, and will
         * be actionable.
         */
        PICTURE_REVIEW
    }

    /**
     * This class encapsulates the user ID and selected share method for a user selected by the
     * {@link FaceLayoutManager}
     */
    public static class SelectedUser {
        public final String id;
        public final UserDatabase.UserRecord userRecord;
        public final UserDatabase.ShareMethod shareMethod;

        private SelectedUser(String id, UserDatabase.UserRecord userRecord,
                             UserDatabase.ShareMethod shareMethod) {
            this.id = id;
            this.userRecord = userRecord;
            this.shareMethod = shareMethod;
        }
    }

    private static final String TAG = "SocialCamera";
    private static final int[] CONFIDENCE_LOW = new int[] { R.attr.state_fr_confidence_low };
    private static final int[] CONFIDENCE_MEDIUM = new int[] { R.attr.state_fr_confidence_medium };
    private static final int[] CONFIDENCE_HIGH = new int[] { R.attr.state_fr_confidence_high };
    private static final int[] ACTION_EMAIL = new int[] { R.attr.state_fr_name_tag_action_email };
    private static final int[] ACTION_SMS = new int[] { R.attr.state_fr_name_tag_action_sms };

    private final FrameLayout layout;
    private final Mode mode;
    private final UserDatabase userDatabase;
    private final boolean isVerboseLoggingEnabled;
    private final LayoutInflater inflater;
    private final CharSequence unrecognizedFaceName;
    private final LinkedList<Item> items = new LinkedList<>();
    private CoordinateTranslator coordinateTranslator = CoordinateTranslator.IDENTITY;

    /**
     * Construct a new {@link FaceLayoutManager}
     * @param layout the {@link FrameLayout} to manage
     * @param mode one of the {@link Mode} values
     * @param userDatabase a {@link UserDatabase} to query for details about {@link Face}s
     * @throws NullPointerException if layout or mode are null
     */
    public FaceLayoutManager(FrameLayout layout, Mode mode, UserDatabase userDatabase) {
        Objects.requireNonNull(layout, "layout may not be null");
        Objects.requireNonNull(mode, "mode may not be null");
        Objects.requireNonNull(userDatabase, "userDatabase may not be null");

        this.layout = layout;
        this.mode = mode;
        this.userDatabase = userDatabase;
        inflater = LayoutInflater.from(layout.getContext());

        isVerboseLoggingEnabled = FacePreferences.isVerboseLoggingEnabled(layout.getContext());
        unrecognizedFaceName = layout.getResources().getText(
                R.string.facelayoutmanager_unrecognized_face_name);

        update(null);
    }

    /**
     * Set a {@link CoordinateTranslator} to translate from the {@link Face} coordinate space to
     * the {@link FrameLayout} coordinate space
     * @param c a {@link CoordinateTranslator} for translation between {@link Face} and
     *          {@link FrameLayout} coordinate spaces
     * @throws NullPointerException if c is null
     */
    public void setCoordinateTranslator(CoordinateTranslator c) {
        Objects.requireNonNull(c, "c may not be null");
        coordinateTranslator = c;
        for (Item item : items) {
            repositionItem(item);
        }
    }

    /**
     * Reset the {@link FrameLayout} managed by this {@link FaceLayoutManager}. This will remove
     * all tracked {@link Face} bounding boxes and name tags.
     */
    public void reset() {
        for (Item item : items) {
            destroyItem(item);
        }
        items.clear();
    }

    /**
     * Update the contents of the managed {@link FrameLayout} with the results of face recognition
     * @param faces the set of {@link Face}s produced by face recognition. If null, the
     *              {@link FaceLayoutManager} will discard all current tracked {@link Face}s.
     */
    public void update(Face[] faces) {
        // Special case: if faces is null, remove all items
        if (faces == null) {
            if (!items.isEmpty()) {
                for (Item item : items) {
                    destroyItem(item);
                }
                items.clear();
                onSelectedFacesChanged();
            }
            return;
        }

        boolean changed = false;

        // First, reposition Faces that still exist and remove Faces that have disappeared
        Iterator<Item> it = items.iterator();
        boolean[] used = new boolean[faces.length];
        while (it.hasNext()) {
            Item item = it.next();

            Face face = null;
            for (int i = 0; i < faces.length; i++) {
                if (faces[i].id == item.face.id) {
                    face = faces[i];
                    used[i] = true;
                    break;
                }
            }

            if (face != null) {
                changed |= item.update(face);
                repositionItem(item);
            } else {
                destroyItem(item);
                it.remove();
                changed = true;
            }
        }

        // Next, add Faces that are new
        for (int i = 0; i < faces.length; i++) {
            if (!used[i]) {
                items.add(createItem(faces[i]));
                changed = true;
            }
        }

        if (changed) {
            onSelectedFacesChanged();
        }

        if (isVerboseLoggingEnabled) {
            for (Item item : items) {
                Log.v(TAG, item.toString());
            }
        }
    }

    /**
     * Get an array of Face objects in the frame
     * @return an array of Face objects in the frame
     */
    public ArrayList<Face> getFaces() {
        if (items == null || items.size() <= 0) {
            return null;
        }

        ArrayList<Face> faces = new ArrayList<Face>(items.size());
        int i = 0;
        for (Item item : items) {
            faces.add(item.face);
        }

        return faces;
    }

    /**
     * Get the number of selected users
     * @return the number of selected users
     */
    public int getNumSelectedUsers() {
        int count = 0;
        for (Item item : items) {
            if (item.recognitionResult != null && item.selected) {
                count++;
            }
        }

        return count;
    }

    /**
     * Get an array of {@link SelectedUser}s describing the set of selected users
     * @return a {@link SelectedUser}[] containing the set of selected users
     */
    public SelectedUser[] getSelectedUsers() {
        SelectedUser[] users = new SelectedUser[getNumSelectedUsers()];
        int i = 0;
        for (Item item : items) {
            if (item.recognitionResult != null && item.selected) {
                users[i++] = new SelectedUser(item.recognitionResult.persistentId,
                        item.userRecord, item.shareMethod);
            }
        }

        return users;
    }

    /**
     * {@link FaceLayoutManager} will invoke this method to request that the specified {@link Face}
     * be enrolled. Subclasses are responsible for actually enrolling the {@link Face}, and
     * recording details in their user database. Once the {@link Face} has been enrolled,
     * {@link #update(Face[])} should be invoked to update the {@link FrameLayout} appropriately.
     * <p/><i>Note: this will only be invoked for {@link FaceLayoutManager}s created with
     * {@link Mode#PICTURE_REVIEW}. Only subclasses created for this mode need to implement this
     * method.</i>
     * @param face the {@link Face} to enroll
     */
    protected void enrollFace(Face face) {
        throw new UnsupportedOperationException();
    }

    /**
     * {@link FaceLayoutManager} will invoke this method whenever the set of selected users changes
     * (as reported by {@link #getSelectedUsers()})
     */
    protected abstract void onSelectedFacesChanged();

    private Item createItem(Face face) {
        final View faceBox = inflater.inflate(R.layout.layout_fr_face_box, layout, false);
        final View nameTag = inflater.inflate(R.layout.layout_fr_name_tag, layout, false);

        Item item = new Item(face, faceBox, nameTag);

        nameTag.addOnLayoutChangeListener(nameTagLayoutChangeListener);
        nameTag.setOnClickListener(nameTagClickListener);
        nameTag.setOnLongClickListener(nameTagLongClickListener);
        item.nameTagAction.setOnClickListener(nameTagActionClickListener);

        if (mode == Mode.PREVIEW) {
            item.nameTag.setClickable(false);
            item.nameTag.setLongClickable(false);
            item.nameTagActionContainer.setVisibility(View.GONE);
        } else {
            item.faceBox.setVisibility(View.GONE);
        }

        layout.addView(faceBox);
        layout.addView(nameTag);

        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)nameTag.getLayoutParams();
        lp.gravity = Gravity.START | Gravity.TOP;
        lp.width = FrameLayout.LayoutParams.WRAP_CONTENT;
        lp.height = FrameLayout.LayoutParams.WRAP_CONTENT;
        nameTag.setLayoutParams(lp);

        repositionItem(item);

        return item;
    }

    private void destroyItem(Item item) {
        layout.removeView(item.faceBox);
        layout.removeView(item.nameTag);
    }

    private void repositionItem(Item item) {
        RectF bounds = new RectF(item.face.bounds);
        coordinateTranslator.mapRect(bounds, bounds);

        int width = (int)bounds.width() + item.faceBox.getPaddingStart() +
                item.faceBox.getPaddingEnd();
        int height = (int)bounds.height() + item.faceBox.getPaddingTop() +
                item.faceBox.getPaddingBottom();
        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)item.faceBox.getLayoutParams();
        if (lp.width != width || lp.height != height) {
            lp.gravity = Gravity.START | Gravity.TOP;
            lp.width = width;
            lp.height = height;
            item.faceBox.setLayoutParams(lp);
        }
        item.faceBox.setTranslationX((int)bounds.left - item.faceBox.getPaddingStart());
        item.faceBox.setTranslationY((int)bounds.top - item.faceBox.getPaddingTop());

        item.nameTag.setTranslationX((int)bounds.centerX() - item.nameTag.getWidth() / 2);
        item.nameTag.setTranslationY((int)bounds.top - item.nameTag.getHeight());
    }

    private static int[] scoreToConfidence(float score) {
        if (score >= Face.RecognitionResult.SCORE_THRESHOLD_HIGH) {
            return CONFIDENCE_HIGH;
        } else if (score >= Face.RecognitionResult.SCORE_THRESHOLD_MEDIUM) {
            return CONFIDENCE_MEDIUM;
        } else {
            return CONFIDENCE_LOW;
        }
    }

    private final View.OnLayoutChangeListener nameTagLayoutChangeListener =
            new View.OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right, int bottom,
                                   int oldLeft, int oldTop, int oldRight, int oldBottom) {
            if ((right - left) == (oldRight - oldLeft) && (bottom - top) == (oldBottom - oldTop)) {
                // Size did not change, no need to reposition the name tag
                return;
            }

            repositionItem((Item)v.getTag());
        }
    };

    private final View.OnClickListener nameTagClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Item item = (Item)v.getTag();
            if (item.recognitionResult != null) {
                new IndicateUserPopup(item).show();
            } else {
                enrollFace(item.face);
            }
        }
    };

    private final View.OnLongClickListener nameTagLongClickListener =
            new View.OnLongClickListener() {
        @Override
        public boolean onLongClick(View v) {
            Item item = (Item)v.getTag();
            if (item.recognitionResult != null) {
                item.toggleSelected();
            }
            return true;
        }
    };

    private final View.OnClickListener nameTagActionClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Item item = (Item)v.getTag();
            if (item.recognitionResult != null) {
                new ShareSettingsPopup(layout.getContext(), item).show();
            } else {
                Log.w(TAG, "Illegal attempt to set a sharing action for an unidentified user");
            }
        }
    };

    private class Item {
        private final View faceBox;
        private final View nameTag;
        private final TextView nameTagName;
        private final ImageView nameTagConfidenceIndicator;
        private final View nameTagActionContainer;
        private final ImageView nameTagAction;

        // Primary source of data for this Item
        private Face face;

        // These fields specify hints provided by UI, to override defaults if applicable
        private String userIndicatedId;
        private UserDatabase.ShareMethod userIndicatedShareMethod;

        // These fields represent the current state of this Item
        private Face.RecognitionResult recognitionResult;
        private UserDatabase.UserRecord userRecord;
        private UserDatabase.ShareMethod shareMethod;
        private boolean selected;
        private boolean invalidated;

        private Item(Face face, View faceBox, View nameTag) {
            this.faceBox = faceBox;
            this.nameTag = nameTag;

            nameTagName = (TextView)nameTag.findViewById(R.id.fr_name_tag_name);
            nameTagConfidenceIndicator = (ImageView)nameTag.findViewById(
                    R.id.fr_name_tag_confidence_indicator);
            nameTagActionContainer = nameTag.findViewById(R.id.fr_name_tag_action_container);
            nameTagAction = (ImageView)nameTag.findViewById(R.id.fr_name_tag_action);

            nameTag.setTag(this);
            nameTagAction.setTag(this);

            selected = true;

            invalidated = true;

            update(face);
        }

        private boolean update(Face face) {
            final String lastId = (recognitionResult != null ?
                    recognitionResult.persistentId : null);

            this.face = face;

            // Resolve the recognition result, taking the user-indicated ID into account. Note that
            // recognitionResults can contain multiple instances of a persistent ID, if more than
            // one face is registered to an ID. However, since we always select the first match, and
            // the results are ordered by score, we will always pick the best match for a given ID.
            recognitionResult = null;
            if (face.recognitionResults.length > 0) {
                if (userIndicatedId != null) {
                    for (Face.RecognitionResult result : face.recognitionResults) {
                        if (userIndicatedId.equals(result.persistentId)) {
                            recognitionResult = result;
                            break;
                        }
                    }
                }

                if (recognitionResult == null) {
                    for (Face.RecognitionResult result : face.recognitionResults) {
                        if (result.score >= Face.RecognitionResult.SCORE_THRESHOLD_LOW) {
                            recognitionResult = result;
                            break;
                        }
                    }
                }
            }

            boolean userChanged;
            if (recognitionResult != null) {
                userChanged = !recognitionResult.persistentId.equals(lastId);
                if (userChanged) {
                    userRecord = userDatabase.getUserRecord(recognitionResult.persistentId);
                }
            } else {
                userChanged = (userRecord != null);
                userRecord = null;
            }

            if (userRecord != null) {
                shareMethod = null;
                if (userIndicatedShareMethod != null) {
                    for (UserDatabase.ShareMethod s : userRecord.supportedShareMethods) {
                        if (s == userIndicatedShareMethod) {
                            shareMethod = s;
                            break;
                        }
                    }
                }
                if (shareMethod == null) {
                    shareMethod = userRecord.shareMethod;
                }
                if (shareMethod == null) {
                    shareMethod = UserDatabase.ShareMethod.NONE;
                }
            } else {
                shareMethod = UserDatabase.ShareMethod.NONE;
            }

            if (userChanged || invalidated) {
                invalidated = false;
                reconfigureViews();
            }

            return userChanged;
        }

        private void setSelected(boolean selected) {
            if (selected != this.selected) {
                this.selected = selected;
                onSelectedFacesChanged();
                invalidate();
            }
        }

        private void toggleSelected() {
            setSelected(!selected);
        }

        private void invalidate() {
            invalidated = true;
            update(this.face);
        }

        private void reconfigureViews() {
            final CharSequence tag;
            if (userRecord != null) {
                tag = userRecord.name;
                nameTagConfidenceIndicator.setVisibility(View.VISIBLE);
            } else {
                tag = unrecognizedFaceName;
                nameTagConfidenceIndicator.setVisibility(View.GONE);
            }

            if (!nameTagName.getText().equals(tag)) {
                nameTagName.setText(tag);
            }

            if (mode == Mode.PREVIEW) {
                if (recognitionResult != null) {
                    nameTag.setVisibility(View.VISIBLE);
                } else {
                    nameTag.setVisibility(View.GONE);
                }

                nameTag.setActivated(false);
            } else {
                final int[] actionState;
                switch (shareMethod) {
                    case EMAIL:
                        actionState = ACTION_EMAIL;
                        break;

                    case SMS:
                        actionState = ACTION_SMS;
                        break;

                    case NONE:
                        actionState = null;
                        break;

                    default:
                        throw new IllegalStateException("Unexpected ShareMethod: " + shareMethod);
                }

                nameTag.setActivated(selected && recognitionResult != null);
                nameTagActionContainer.setVisibility(actionState != null ?
                        View.VISIBLE : View.GONE);
                nameTagAction.setImageState(actionState, true);
            }

            nameTagConfidenceIndicator.setImageState(recognitionResult != null ?
                    scoreToConfidence(recognitionResult.score) : null, true);
        }

        @Override
        public String toString() {
            return "Face ID=" + face.id +
                    ", PID=" + (recognitionResult != null ? recognitionResult.persistentId : "-1") +
                    ", Name=" + (userRecord != null ? userRecord.name : "unknown");
        }
    }

    private class IndicateUserPopup {
        private static final int POPUP_DISMISS_DELAY_MS = 250;

        private final Item item;
        private final IndicateUserAdapter adapter;
        private final ListPopupWindow popup;

        private IndicateUserPopup(Item item) {
            this.item = item;

            Context c = item.nameTag.getContext();
            adapter = new IndicateUserAdapter(c, item);
            popup = new ListPopupWindow(c);
            popup.setAnchorView(item.nameTagName);
            popup.setWidth(c.getResources().getDimensionPixelSize(
                    R.dimen.facelayoutmanager_fr_name_popup_width));
            popup.setAdapter(adapter);
            popup.setOnItemClickListener(indicateUserPopupClickListener);
            popup.setBackgroundDrawable(c.getDrawable(R.drawable.fr_popup_background));
        }

        private void show() {
            popup.show();

            ListView list = popup.getListView();
            list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
            list.setItemChecked(0, true);
        }

        private final AdapterView.OnItemClickListener indicateUserPopupClickListener =
                new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                IndicateUserAdapter.Record record = adapter.getItem(position);

                switch (record.type) {
                    case IndicateUserAdapter.Record.TYPE_RESULT:
                        if (record.result.persistentId.equals(
                                item.recognitionResult.persistentId)) {
                            // Currently selected user, toggle selection state
                            item.toggleSelected();
                        } else {
                            // Newly selected user, set selected
                            item.setSelected(true);
                        }

                        item.userIndicatedId = record.result.persistentId;
                        item.invalidate();

                        adapter.notifyDataSetChanged(); // recreate views in list with new state
                        break;

                    case IndicateUserAdapter.Record.TYPE_ENROLL_OTHER_CONTACT:
                        // N.B. Clear the indicated ID (if any). The newly enrolled face should be
                        // the new best match.
                        item.userIndicatedId = null;
                        item.setSelected(true);
                        item.invalidate();

                        enrollFace(item.face);
                        break;

                    default:
                        throw new IllegalStateException("Unexpected type: " + record.type);
                }

                layout.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        popup.dismiss();
                    }
                }, POPUP_DISMISS_DELAY_MS);
            }
        };

        private class IndicateUserAdapter extends ArrayAdapter<IndicateUserAdapter.Record> {
            private final LayoutInflater inflater;

            public IndicateUserAdapter(Context context, Item item) {
                super(context, R.layout.layout_fr_name_dropdown_item, R.id.fr_name_popup_item_name);

                Objects.requireNonNull(item, "item may not be null");

                inflater = LayoutInflater.from(context);

                // N.B. The recognition results can contain multiple results with the same
                // persistent ID, if more than one face are registered to an ID. Since the results
                // are ordered by score, we can safely skip subsequent occurrences of an ID.
                HashSet<String> ids = new HashSet<>();
                for (Face.RecognitionResult result : item.face.recognitionResults) {
                    if (result.score >= Face.RecognitionResult.SCORE_THRESHOLD_LOW &&
                            !ids.contains(result.persistentId)) {
                        add(new Record(Record.TYPE_RESULT, result));
                        ids.add(result.persistentId);
                    }
                }

                add(new Record(Record.TYPE_ENROLL_OTHER_CONTACT, null));

                sort(new RecordComparer());
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                if (convertView == null) {
                    convertView = inflater.inflate(R.layout.layout_fr_name_dropdown_item, parent,
                            false);
                }

                final TextView nameTextView = (TextView)convertView.findViewById(
                        R.id.fr_name_popup_item_name);
                final ImageView confidenceIndicator = (ImageView)convertView.findViewById(
                        R.id.fr_name_popup_item_confidence_indicator);

                final Record record = getItem(position);

                switch (record.type) {
                    case Record.TYPE_RESULT:
                        nameTextView.setText(userDatabase.getUserRecord(record.result.persistentId)
                                .name);
                        confidenceIndicator.setImageState(scoreToConfidence(record.result.score),
                                true);
                        confidenceIndicator.setVisibility(View.VISIBLE);
                        convertView.setBackgroundResource(item.selected ?
                                R.drawable.fr_popup_item_background_selected :
                                R.drawable.fr_popup_item_background_unselected);
                        break;

                    case Record.TYPE_ENROLL_OTHER_CONTACT:
                        nameTextView.setText(R.string.facelayoutmanager_enroll_other_user);
                        confidenceIndicator.setVisibility(View.GONE);
                        convertView.setBackgroundResource(R.drawable.fr_popup_item_background_selected);
                        break;

                    default:
                        throw new IllegalStateException("Unexpected type: " + record.type);
                }

                return convertView;
            }

            protected class Record {
                private static final int TYPE_RESULT = 0;
                private static final int TYPE_ENROLL_OTHER_CONTACT = 1;

                private final int type;
                private final Face.RecognitionResult result;

                private Record(int type, Face.RecognitionResult result) {
                    this.type = type;
                    this.result = result;
                }
            }

            private class RecordComparer implements Comparator<Record> {
                @Override
                public int compare(Record r1, Record r2) {
                    if (r1 == r2) {
                        return 0;
                    } else if (r1 == null || r2 == null) {
                        return -1;
                    } else if (r1.type != r2.type) {
                        return (r1.type - r2.type);
                    } else if (r1.type == Record.TYPE_RESULT) {
                        // N.B. Name is just a cached value, don't include it in comparison
                        if (item.recognitionResult == r1.result) {
                            return -1;
                        } else if (item.recognitionResult == r2.result) {
                            return 1;
                        } else {
                            float scoreDelta = r2.result.score - r1.result.score;
                            return (int)Math.signum(scoreDelta);
                        }
                    } else {
                        return 0;
                    }
                }
            }
        }
    }

    private class ShareSettingsPopup {
        private final Context context;
        private final LayoutInflater inflater;
        private final Item item;
        private final PopupWindow popup;

        private ListView list;
        private Button alwaysButton;
        private Button justOnceButton;

        private ShareSettingsPopup(Context context, Item item) {
            this.context = new ContextThemeWrapper(context, R.style.FRShareSettingsPopupTheme);
            this.inflater = LayoutInflater.from(this.context);
            this.item = item;
            popup = new PopupWindow(this.context);
        }

        public void show() {
            popup.dismiss();

            ShareMethodAdapter adapter = new ShareMethodAdapter(context,
                    item.userRecord.supportedShareMethods);
            Size listViewContentSize = adapter.estimateListViewContentSize();

            @SuppressLint("InflateParams")
            View contentView = inflater.inflate(R.layout.layout_fr_share_settings, null, false);

            // N.B. Does not account for ListView padding or dividers, but we don't use them here
            list = (ListView)contentView.findViewById(android.R.id.list);
            ViewGroup.LayoutParams lp = list.getLayoutParams();
            lp.height = listViewContentSize.getHeight();
            list.setLayoutParams(lp);
            list.setMinimumWidth(listViewContentSize.getWidth());
            list.setAdapter(adapter);
            list.setOnItemClickListener(shareModeClickedListener);
            list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);

            justOnceButton = (Button)contentView.findViewById(R.id.fr_share_settings_just_once);
            alwaysButton = (Button)contentView.findViewById(R.id.fr_share_settings_always);
            justOnceButton.setOnClickListener(shareButtonClickedListener);
            alwaysButton.setOnClickListener(shareButtonClickedListener);

            updateUi();

            popup.setContentView(contentView);
            popup.setFocusable(true);
            popup.setWindowLayoutMode(ViewGroup.LayoutParams.WRAP_CONTENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
            popup.showAtLocation(layout, Gravity.CENTER, 0, 0);

            // Set popup window size , and add dim behind flag to popup
            View root = contentView.getRootView();
            final int spec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
            root.measure(spec, spec);
            Size rootSize = new Size(root.getMeasuredWidth(), root.getMeasuredHeight());
            WindowManager wm = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            WindowManager.LayoutParams wlp = (WindowManager.LayoutParams)root.getLayoutParams();
            wlp.flags |= WindowManager.LayoutParams.FLAG_DIM_BEHIND;
            TypedArray a = context.obtainStyledAttributes(new int[] {
                    android.R.attr.backgroundDimAmount });
            wlp.dimAmount = a.getFloat(0, 0f);
            wlp.width = rootSize.getWidth();
            wlp.height = rootSize.getHeight();
            a.recycle();
            wm.updateViewLayout(root, wlp);
        }

        private ListView.OnItemClickListener shareModeClickedListener =
                new ListView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                updateUi();
            }
        };

        private View.OnClickListener shareButtonClickedListener =
                new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final int pos = list.getCheckedItemPosition();
                if (pos == ListView.INVALID_POSITION) {
                    Log.w(TAG, "Share button clicked without an activated share method");
                    return;
                }

                UserDatabase.ShareMethod shareMethod = ((ShareMethodAdapter)list.getAdapter())
                        .getItem(pos);
                item.userIndicatedShareMethod = shareMethod;
                item.invalidate();

                if (v == alwaysButton) {
                    userDatabase.updateUser(item.recognitionResult.persistentId, shareMethod);
                }

                popup.dismiss();
            }
        };

        private void updateUi() {
            boolean en = (list.getCheckedItemPosition() != ListView.INVALID_POSITION);
            alwaysButton.setEnabled(en);
            justOnceButton.setEnabled(en);
        }

        private class ShareMethodAdapter extends ArrayAdapter<UserDatabase.ShareMethod> {
            public ShareMethodAdapter(Context context, UserDatabase.ShareMethod[] objects) {
                super(context, 0, objects);
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                final TextView view;
                if (convertView != null) {
                    view = (TextView)convertView;
                } else {
                    view = (TextView)inflater.inflate(R.layout.layout_fr_share_settings_item,
                            parent, false);
                }

                UserDatabase.ShareMethod shareMethod = getItem(position);

                switch (shareMethod) {
                    case EMAIL:
                        view.setText(R.string.fr_share_settings_share_method_email);
                        view.setCompoundDrawablesRelativeWithIntrinsicBounds(context.getDrawable(
                                R.drawable.ic_bottom_sheet_email), null, null, null);
                        break;

                    case SMS:
                        view.setText(R.string.fr_share_settings_share_method_sms);
                        view.setCompoundDrawablesRelativeWithIntrinsicBounds(context.getDrawable(
                                R.drawable.ic_bottom_sheet_sms), null, null, null);
                        break;

                    default:
                        throw new IllegalStateException("Unexpected ShareMethod: " + shareMethod);
                }

                return view;
            }

            public Size estimateListViewContentSize() {
                FrameLayout representativeLayout = new FrameLayout(context);
                representativeLayout.addView(getView(0, null, representativeLayout));
                final int spec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
                representativeLayout.measure(spec, spec);
                return new Size(representativeLayout.getMeasuredWidth(),
                        representativeLayout.getMeasuredHeight() * getCount());
            }
        }
    }
}
