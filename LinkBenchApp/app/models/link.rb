class Link < ActiveRecord::Base
	validates :id2, :linktype, :visibility, :data, :version, :time, presence: true
	validates_uniqueness_of :node_id, :scope => [:id2, :linktype]   # composite uniquness
	belongs_to :friendis, class_name: "Node"
	belongs_to :friendof, class_name: "Node"
	belongs_to :node

  $VISIBILITY_HIDDEN = 0
  $VISIBILITY_DEFAULT = 1

  after_save :new_count
  after_update :update_count
  after_destroy :destroy_modify_count

  @count_incr = 0
  @expunge_set = false

	def new_count
		ct = Count.find_by(node_id: :node_id, link_type: :linktype)
		if ct.nil?
			ct = Count.new(node_id: self.node_id, link_type: self.linktype, 
				version: self.version, time: Time.now, countval: 0)
			ct.save
    else
      ct.update(countval: ct.countval + 1,
                version: ct.version + 1, time: Time.now)
		end
	end

  def update_count
    ct = Count.find_by(node_id: self.node_id, link_type: self.linktype)
    if ct.nil?
      ct = Count.new(node_id: self.node_id, link_type: self.linktype,
                     version: 0, time: Time.now, countval: @count_incr)
      ct.save
    else
      if @expunge_set
        ct.update(countval: ct.countval == 0 ? 0 : countval - 1,
                  version: ct.version + 1, time: Time.now)
      elsif @count_incr >= 0
        ct.update(countval: ct.countval + @count_incr,
                  version: ct.version + 1, time: Time.now)
      end
    end
  end

	def destroy_modify_count
		ct = Count.find_by(node_id: :node_id, link_type: :linktype)
		if ct.nil?
			ct = Count.new(node_id: self.node_id, link_type: self.linktype, 
				version: 0, time: Time.now, countval: 0)
			ct.save
		else
      ct.update(countval: ct.countval == 0 ? 0 : countval - 1,
                version: ct.version + 1, time: Time.now)
		end
	end

	# Called for link deletion
	def link_delete(lid1, lid2, llinktype, expunge)
    Link.transaction do
      lt = Link.find_by(node_id: lid1, id2: lid2, linktype: llinktype)
      if expunge
        lt.destroy
      elsif lt.visibility != $VISIBILITY_HIDDEN
        @expunge_set = true
        lt.update(visibility: $VISIBILITY_HIDDEN)
      end
    end
  end

  def addLink(lid1, lid2, llinktype, lvisibility, ldata, lversion, ltime)
    Link.transaction do
      lt = Link.find_by(node_id: lid1, id2: lid2, linktype: llinktype)
      if lt.nil?
        lt = Link.new(node_id: lid1, id2: lid2, linktype: llinktype,
                      version: lversion, visibility: lvisibility, data: ldata, time: ltime)
        lt.save
      else
        if lvisibility == lt.visibility   #Visibility remains unchanged
          @count_incr = -1
          lt.update(visibility: lvisibility, version: lversion, data: ldata, time: ltime)
        else
          if lvisibility == $VISIBILITY_DEFAULT
            @count_incr = 1
          else
            @count_incr = 0
          end
          lt.update(visibility: lvisibility, version: lversion, data: ldata, time: ltime)
        end
      end
    end
  end

end
